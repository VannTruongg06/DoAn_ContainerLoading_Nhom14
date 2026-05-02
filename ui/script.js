import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

// --- Cấu hình Scene ---
const container = document.getElementById('canvas-container');
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x0f172a); // Màu nền tối (Slate-900)

// Camera
const camera = new THREE.PerspectiveCamera(75, container.clientWidth / container.clientHeight, 0.1, 1000);
camera.position.set(10, 10, 10);

// Renderer
const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize(container.clientWidth, container.clientHeight);
renderer.shadowMap.enabled = true;
container.appendChild(renderer.domElement);

// Điều khiển camera (Zoom, Xoay, Kéo)
const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;

// --- Ánh sáng ---
const ambientLight = new THREE.AmbientLight(0xffffff, 0.6); // Ánh sáng môi trường
scene.add(ambientLight);

const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
directionalLight.position.set(20, 50, 30);
directionalLight.castShadow = true;
scene.add(directionalLight);

// Trục tọa độ (Giúp định hướng: Đỏ=X, Xanh lá=Y, Xanh dương=Z)
const axesHelper = new THREE.AxesHelper(5);
scene.add(axesHelper);

// Grid nền cho đẹp
const gridHelper = new THREE.GridHelper(50, 50, 0x334155, 0x1e293b);
gridHelper.position.y = -0.01;
scene.add(gridHelper);

// --- Các biến quản lý Object ---
let containerBox = null;
let itemMeshes = [];

// --- Xử lý sự kiện đọc File ---
document.getElementById('fileInput').addEventListener('change', function(e) {
    const file = e.target.files[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = function(e) {
        try {
            const data = JSON.parse(e.target.result);
            renderSimulation(data);
        } catch (err) {
            alert('Lỗi: File không đúng định dạng JSON!');
            console.error(err);
        }
    };
    reader.readAsText(file);
});

// --- Hàm Render Chính ---
function renderSimulation(data) {
    // 1. Xóa các object cũ
    if (containerBox) scene.remove(containerBox);
    itemMeshes.forEach(m => scene.remove(m));
    itemMeshes = [];

    // 2. Cập nhật UI Sidebar
    document.getElementById('info-algo').textContent = data.algorithm || 'N/A';
    document.getElementById('info-size').textContent = `${data.container.width}x${data.container.height}x${data.container.depth}`;
    document.getElementById('info-fill').textContent = (data.fill_rate || 0).toFixed(2) + '%';
    document.getElementById('fill-bar').style.width = (data.fill_rate || 0) + '%';
    document.getElementById('info-count').textContent = data.items.length;

    // 3. Vẽ Container (Wireframe)
    const cw = data.container.width;
    const ch = data.container.height;
    const cd = data.container.depth;

    const containerGeo = new THREE.BoxGeometry(cw, ch, cd);
    const containerMat = new THREE.MeshBasicMaterial({ 
        color: 0x60a5fa, 
        wireframe: true,
        transparent: true,
        opacity: 0.3 
    });
    containerBox = new THREE.Mesh(containerGeo, containerMat);
    
    // Lưu ý: Three.js đặt tâm ở (0,0,0), nên dời tâm để góc (0,0,0) nằm ở góc dưới
    containerBox.position.set(cw/2, ch/2, cd/2);
    scene.add(containerBox);

    // 4. Vẽ Items
    data.items.forEach(item => {
        // Kích thước: w (x), h (y), d (z)
        const geometry = new THREE.BoxGeometry(item.w, item.h, item.d);
        const material = new THREE.MeshPhongMaterial({ 
            color: item.color || '#3b82f6',
            transparent: true,
            opacity: 0.9,
            shininess: 100
        });
        
        const mesh = new THREE.Mesh(geometry, material);

        // Hiệu chỉnh tọa độ: x_three = x_json + w/2
        // Vì tọa độ trong C++ thường là tọa độ góc (bottom-left-front)
        mesh.position.set(
            item.x + item.w / 2,
            item.y + item.h / 2,
            item.z + item.d / 2
        );

        // Thêm viền cho khối hộp để nhìn rõ khối
        const edges = new THREE.EdgesGeometry(geometry);
        const line = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: 0x000000, opacity: 0.5, transparent: true }));
        mesh.add(line);

        mesh.castShadow = true;
        mesh.receiveShadow = true;
        
        scene.add(mesh);
        itemMeshes.push(mesh);
    });

    // 5. Tự động điều chỉnh Camera nhìn vào khối
    const maxDim = Math.max(cw, ch, cd);
    camera.position.set(cw * 1.5, ch * 1.5, cd * 1.5);
    controls.target.set(cw/2, ch/2, cd/2);
    controls.update();
}

// Nút Reset Camera
document.getElementById('resetCamera').onclick = () => {
    if (containerBox) {
        const cw = containerBox.geometry.parameters.width;
        const ch = containerBox.geometry.parameters.height;
        const cd = containerBox.geometry.parameters.depth;
        camera.position.set(cw * 1.5, ch * 1.5, cd * 1.5);
        controls.target.set(cw/2, ch/2, cd/2);
    } else {
        camera.position.set(10, 10, 10);
        controls.target.set(0, 0, 0);
    }
    controls.update();
};

// --- Loop render ---
function animate() {
    requestAnimationFrame(animate);
    controls.update();
    renderer.render(scene, camera);
}

// Xử lý resize cửa sổ
window.addEventListener('resize', () => {
    camera.aspect = container.clientWidth / container.clientHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(container.clientWidth, container.clientHeight);
});

animate();
